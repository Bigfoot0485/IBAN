<?php
/**
 * Description of IBAN_DE
 *
 * @author sven
 */
require_once 'IBAN_DEFAULT.inc';

class IBAN_DE extends IBAN_DEFAULT {
    
    const BLZ_FILENAME = 'blz.txt';
    const EXHYPORANGES_FILENAME = 'exhypo-ranges.xml';
    
    private $_regexp = "/^DE\d{20}$/";
    
    function __construct() {
        parent::__construct("DE");
    }
    
    private function _getValueOfBankleitzahldatei( $blz, $start, $length ) {
        
        // read out the blz.txt to find out, which rule applies on the
        // particular BLZ
        $blz_rows = file(IBAN_DE::BLZ_FILENAME, FILE_USE_INCLUDE_PATH + FILE_IGNORE_NEW_LINES );
        if( $blz_rows === FALSE )
            throw new InvalidArgumentException( sprintf( "The file %s is necessary for German IBAN creation, but couldn't be read out.", IBAN_DE::BLZ_FILENAME ) );
        
        // check each row, if tells, which rule is to be choosen 
        foreach( $blz_rows as $row ) {
            
            // first, ensure that the row is a valid for the purpose and it's
            // structure isn't been changed in the meantime
            if( preg_match( '/^\d{8}.{160}\d{6}$/', $row ) === FALSE )
                throw new InvalidArgumentException( sprintf( "The file %s is containing invalid rows.", IBAN_DE::BLZ_FILENAME ) );
            
            if( substr( $row, 0, 8 ) === $blz ) {
                
                $rule = substr( $row, $start, $length );
                
                return( $rule );
                
            }
            
        }
        
        throw new InvalidArgumentException("You have passed an invalid german BLZ.");
        
    }
    
    /**
     * return whether the account has a valid checksum, according to the BLZ-
     * specific checksum-routine
     * @todo For the first version, I expect the accounts to be valid in general
     * @param type $blz
     * @param type $account
     * @return boolean
     */
    private function _isValidAccount( $blz, $account ) {
        return ( TRUE );
    }
    
    private function _chooseRule( $blz ) {
        
        return( $this->_getValueOfBankleitzahldatei( $blz, -6, 6 ) );
        
    }
    
    private function _isRemoveFlagSet( $blz ) {
        
        if( $this->_getValueOfBankleitzahldatei( $blz, -15, 1 ) == '0' )
                return( FALSE );
        
        if( $this->_getValueOfBankleitzahldatei( $blz, -15, 1 ) == '1' )
                return( TRUE );
        
        throw new exception( "The Remove Flag has an unexpected state" );
        
    }
    
    /**
     * If some BLZ are marked for deletion, there is a following BLZ set
     * in the Bankleitzahlendatei. This function searches for the following
     * BLZ according to the Bankleitzahlendatei.
     * If there is no following one, it return FALSE. Basically the program code
     * shouldn't access this method, if there isn't one. So the calling method
     * should handle the FALSE and - maybe - trigger an exception.
     * @param type $blz
     */
    private function _getFollowingBLZ( $blz ) {
        
        $return = $this->_getValueOfBankleitzahldatei( $blz, -14, 8 );
        
        if( $return === "00000000" ) return( FALSE );
        
        return( $return );
        
    }
    
    /**
     * 
     * @param type $range
     * @return type
     * @throws Exception
     * @todo Die Datei exhypo-ranges.xml muss noch fertig gepflegt werden...
     */
    private function _getKontokreiseExHypo( $range ) {
        
        $query = sprintf( '/hypo_ranges/range[@id=%d]', $range );
        
        $xml = new DOMDocument;
        $xml->preserveWhiteSpace = false;

        $content = file_get_contents( IBAN_DE::EXHYPORANGES_FILENAME, true );
        $xml->loadXML( $content );

        $xpath = new DOMXPath($xml);
        $obj = $xpath->query($query);
        
        if( $obj->length == 1 ) {
            $use_blz = $obj->item(0)->textContent;
            return( $use_blz );
        } else {
            throw new Exception( "The file exhypo-ranges.xml doesn't contain the range for this account" );
        }
        
    }
    
    /**
     * @return bool returns TRUE if it is allowed to generate an IBAN at all
     */
    private function _prepareParametersForIBAN( &$account, &$blz ) {
        
        $account_backup = $account;
        $blz_backup = $blz;
        
        $rule = $this->_chooseRule($blz);
        $rule_revision = '00';
        
        switch( substr( $rule, 0, 4 ) ) {
                
            case "0000":
                break;
            
            case "0002":
                if( substr( $account, -3, 2 ) =="86"
                        || substr( $account, -3, 1 ) =="6" ) {
                    return false;
                }
                break;
            
            case "0003":
                if( $account == "6161604670" ) {
                    return false;
                }
                break;
            
            case "0004":
                if( $account == "135" ) $account = "0990021440";
                if( $account == "111" ) $account = "6600012020";
                if( $account == "1900" ) $account = "0920019005";
                if( $account == "7878" ) $account = "0780008006";
                if( $account == "8888" ) $account = "0250030942";
                if( $account == "9595" ) $account = "1653524703";
                if( $account == "97097" ) $account = "0013044150";
                if( $account == "112233" ) $account = "0630025819";
                if( $account == "336666" ) $account = "6604058903";
                if( $account == "484848" ) $account = "0920018963";
                break;
            
            case "0007":
                $rule_revision = '00';
                $account_map = array("111" => "1115", "221" => "23002157", "1888" => "18882068",
                    "2006" => "1900668508", "2626" => "1900730100", "3004" => "1900637016", "3636" => "23002447",
                    "4000" => "4028", "4444" => "17368", "5050" => "73999", "8888" => "1901335750",
                    "30000" => "9992959", "43430" => "1901693331", "46664" => "1900399856", "55555" => "34407379",
                    "102030" => "1900480466", "151515" => "57762957", "222222" => "2222222", "300000" => "9992959",
                    "333333" => "33217", "414141" => "92817", "606060" => "91025", "909090" => "90944",
                    "2602024" => "5602024", "3000000" => "9992959", "7777777" => "2222222", "8090100" => "38901",
                    "14141414" => "43597665", "15000023" => "15002223", "15151515" => "57762957", "22222222" => "2222222",
                    "200820082" => "1901783868", "222220022" => "2222222" );
                
                if( array_key_exists($account, $account_map) ) {
                    $account = $account_map[$account];
                }
                break;
            
            case "0008":
                $rule_revision = '00';
                if( in_array( $blz, array( "10020200", "20120200", "25020200",
                    "30020500", "51020000", "55020000", "60120200", "70220200",
                    "86020200" ) ) ) {
                    $blz = "50020200";
                }
                break;
            
            case "0009":
                $rule_revision = '00';
                if( strlen( $account ) == 10 && substr( $account, 0, 4 ) == "1116" ) {
                    $account = sprintf( "3047%s", substr( $account, 4 ) );
                }
                $blz = "68351557";
                break;
            
            case "0010":
                $rule_revision = '00';
                if( $account == "2000" ) $account = "222000";
                if( $account == "800000" ) $account = "180802";
                break;
            
            case "0011":
                $rule_revision = '00';
                if( $account == "1000" ) $account = "8010001";
                if( $account == "47800" ) $account = "47803";
                break;
            
            case "0012":
                $rule_revision = '00';
                $blz = "50050000";
                break;
            
            case "0013":
                $rule_revision = '00';
                $blz = "30050000";
                break;
            
            case "0014":
                $rule_revision = '00';
                $blz = "30060601";
                break;
            
            case "0015":
                $rule_revision = '00';
                if( $account == "556" ) $account = "0000101010";
                if( $account == "888" ) $account = "0031870011";
                if( $account == "4040" ) $account = "4003600101";
                if( $account == "5826" ) $account = "1015826017";
                if( $account == "25000" ) $account = "0025000110";
                if( $account == "393393" ) $account = "0033013019";
                if( $account == "444555" ) $account = "0032230016";
                if( $account == "603060" ) $account = "6002919018";
                if( $account == "2120041" ) $account = "0002130041";
                if( $account == "80868086" ) $account = "4007375013";
                if( $account == "400569017" ) $account = "4000569017";
                break;
            
            case "0016":
                $rule_revision = '00';
                if( $account == "300000" ) $account = "18128012";
                break;
            
            case "0017":
                $rule_revision = '00';
                if( $account == "100" ) $account = "2009090013";
                if( $account == "111" ) $account = "2111111017";
                if( $account == "240" ) $account = "2100240010";
                if( $account == "4004" ) $account = "2204004016";
                if( $account == "4444" ) $account = "2044444014";
                if( $account == "6060" ) $account = "2016060014";
                if( $account == "102030" ) $account = "1102030016";
                if( $account == "333333" ) $account = "2033333016";
                if( $account == "909090" ) $account = "2009090013";
                if( $account == "50005000" ) $account = "5000500013";
                break;
            
            case "0018":
                $rule_revision = '00';
                if( $blz == 39060180 ) {
                    if( $account == "556" ) $account = "120440110";
                    if( $account == "5435435430" ) $account = "543543543";
                    if( $account == "2157" ) $account = "121787016";
                    if( $account == "9800" ) $account = "120800019";
                    if( $account == "202050" ) $account = "1221864014";
                }
                break;
            
            case "0019":
                $rule_revision = '00';
                if( in_array( $blz, array("50130100", "50220200", "70030800") ) )
                        $blz = "50120383";
                break;
                
            /**
             * @todo: Not ready yet.
             */
            case "0020":
                $rule_revision = '00';
                if( $blz == "10020000" ) return FALSE;
                if( $account == "9999" ) $account = "92777202";
                if( strlen( $account ) < 7 || strlen( $account ) > 9 ) return FALSE;
                break;
            
            case "0021":
                $rule_revision = '00';
                $blz = "36020030";
                break;
            
            case "0022":
                $rule_revision = '00';
                if( $blz == "43060967" && $account == "1111111" ) $account = "2222200000";
                break;
            
            case "0023":
                $rule_revision = '00';
                if( $blz == "26590025" && $account == "700" ) $account = "1000700800";
                break;
            
            case "0024":
                $rule_revision = '00';
                if( $blz == "36060295" && $account == "94" ) $account = "1694";
                if( $blz == "36060295" && $account == "248" ) $account = "17248";
                if( $blz == "36060295" && $account == "345" ) $account = "17345";
                if( $blz == "36060295" && $account == "400" ) $account = "14400";
                break;
            
            case "0025":
                $rule_revision = '00';
                $blz = "60050101";
                break;
            
            case "0026":
                $rule_revision = '00';
                /**
                 * @todo Implement the validating of (SOME) institutes
                 * Für die Bankleitzahl 35060190 erfolgt die IBAN-Berechnung 
                 * nach der Standard-IBAN-Regel. Abweichend hierzu kann aber für
                 * nachfolgend aufgeführte Konten die IBAN-Berechnung
                 * durchgeführt werden, auch wenn die Kontrolle der in der
                 * Kontonummer enthaltenen Prüfziffer fehlschlägt:
                 * 55111, 8090100
                 */
                break;
            
            case "0027":
                $rule_revision = '00';
                /**
                 * @todo Implement the validating of (SOME) institutes
                 * Für die Bankleitzahl 32060362 erfolgt die IBAN-Berechnung 
                 * nach der Standard-IBAN-Regel. Abweichend hierzu kann aber für
                 * nachfolgend aufgeführte Konten die IBAN-Berechnung
                 * durchgeführt werden, auch wenn die Kontrolle der in der
                 * Kontonummer enthaltenen Prüfziffer fehlschlägt:
                 * 3333, 4444
                 */
                break;
            
            case "0028":
                $rule_revision = '00';
                if( $blz == "25050299" ) $blz = "25050180";
                break;
            
            case "0029":
                $rule_revision = '00';
                if( strlen( $account ) == 10 && $account{0} != "0" && $account{3} == "0" )
                    $account = '0' . substr( $account, 0, 3 ) . substr( $account, -6 );
                break;
            
            case "0030":
                $rule_revision = '00';
                /**
                 * @todo Implement the validating of (SOME) institutes
                 * Für die Bankleitzahl 32060362 erfolgt die IBAN-Berechnung 
                 * nach der Standard-IBAN-Regel. Abweichend hierzu kann aber für
                 * nachfolgend aufgeführte Konten die IBAN-Berechnung
                 * durchgeführt werden, auch wenn die Kontrolle der in der
                 * Kontonummer enthaltenen Prüfziffer fehlschlägt:
                 * ...
                 */
                break;
            
            case "0031":
                $rule_revision = '00';
                
                if( $this->_isRemoveFlagSet( $blz ) === TRUE ) {
                    
                    if( strlen( $account ) == 10 ) {
                        if( $this->_isValidAccount( $blz, $account ) === true ) {
                            $blz = $this->_getKontokreiseExHypo( substr( $account, 0, 3 ) );
                        }
                    }
                
                    else return( FALSE );
                }
                
                break;
            
            case "0032":
                $rule_revision = '00';
                
                if( $this->_isRemoveFlagSet( $blz ) === FALSE ) {
                    
                    if( $this->_isValidAccount( $blz, $account ) === true ) {

                        if( strlen( $account ) == 10 ) {
                            
                            $blz = $this->_getKontokreiseExHypo( substr( $account, 0, 3 ) );
                            
                        } else {
                            
                            if( (int)$account >= 800000000 && (int)$account <= 899999999 ) {
                                return( FALSE );
                            }
                            
                            return( true );
                            
                        }

                    } else {
                        
                        throw new exception( "The check digit of the account is wrong!" );
                        
                    }
                    
                }
                
                break;
            
            case "0033":
                $rule_revision = '00';
                
                if( $this->_isRemoveFlagSet( $blz ) === FALSE ) {
                    
                    if( $this->_isValidAccount( $blz, $account ) === true ) {

                        if( strlen( $account ) == 10 ) {
                            
                            $blz = $this->_getKontokreiseExHypo( substr( $account, 0, 3 ) );
                            
                        }

                    } else {
                        
                        throw new exception( "The check digit of the account is wrong!" );
                        
                    }
                    
                }
                
                if( $account == 22222 ) $account = 5803435253;
                if( $account == 1111111 ) $account = 39908140;
                if( $account == 94 ) $account = 2711931;
                if( $account == 7777777 ) $account = 5800522694;
                if( $account == 55555 ) $account = 5801800000;
                
                break;
                
            case "0034":
                $rule_revision = '00';
                
                if( $this->_isRemoveFlagSet( $blz ) === FALSE ) {
                    
                    if( $this->_isValidAccount( $blz, $account ) === true ) {

                        if( strlen( $account ) == 10 ) {
                            
                            $blz = $this->_getKontokreiseExHypo( substr( $account, 0, 3 ) );
                            
                        } else {
                            
                            if( (int)$account >= 800000000
                            &&  (int)$account <= 899999999 ) {
                                return( FALSE );
                            }
                            
                        }

                    } else {
                        
                        throw new exception( "The check digit of the account is wrong!" );
                        
                    }
                    
                }
                
                if( $account == 500500500 ) $account = 4340111112;
                if( $account == 502 ) $account = 4340118001;
                
                break;
                
            case "0035":
                $rule_revision = '00';
                
                if( $this->_isRemoveFlagSet( $blz ) === FALSE ) {
                    
                    if( $this->_isValidAccount( $blz, $account ) === true ) {

                        if( strlen( $account ) == 10 ) {
                            
                            $blz = $this->_getKontokreiseExHypo( substr( $account, 0, 3 ) );
                            
                        } else {
                            
                            if( (int)$account >= 800000000
                            &&  (int)$account <= 899999999 ) {
                                return( FALSE );
                            }
                            
                        }

                    } else {
                        
                        throw new exception( "The check digit of the account is wrong!" );
                        
                    }
                    
                }
                
                if( $account == 9696 ) $account = 1490196966;
                
                break;
            
            case "0036":
                $rule_revision = '00';
                /**
                 * @todo Als erstes muss immer die Kontonummer anhand des in
                 * Banleitzahlenband hinterlegten Prüfzifferverfahrens geprüft
                 * werden. Nur bei einer korrekten Kontonummer sind die
                 * folgenden Berechnungsvarianten gültig.
                 */
                
                // Variante 1
                if( (int)$account >= 100000 && (int)$account <= 899999 ) {
                    $blz = "21050000";
                    $account = sprintf( "%s000", (int)$account );
                }
                
                // Variante 2
                if( (int)$account >= 30000000 && (int)$account <= 59999999 
                        || (int)$account >= 100000000 && (int)$account <= 899999999
                        || (int)$account >=1000000000 && (int)$account <=1999999999
                        || (int)$account >=3000000000 && (int)$account <=3999999999
                        || (int)$account >=4000000000 && (int)$account <=6999999999
                        || (int)$account >=7000000000 && (int)$account <=7099999999
                        || (int)$account >=8500000000 && (int)$account <=8599999999
                        || (int)$account >=9000000000 && (int)$account <=9999999999) {
                    $blz = "21050000";
                }
                
                // Variante 3
                if( (int)$account >= 0 && (int)$account <= 99999 
                        || (int)$account >= 900000 && (int)$account <= 29999999
                        || (int)$account >=60000000 && (int)$account <=99999999
                        || (int)$account >=900000000 && (int)$account <=999999999
                        || (int)$account >=2000000000 && (int)$account <=2999999999
                        || (int)$account >=7100000000 && (int)$account <=8499999999
                        || (int)$account >=8600000000 && (int)$account <=8999999999) {
                    return false;
                }
                break;
            
            case "0037":
                $rule_revision = '00';
                if( $blz == "20110700" ) $blz = "30010700";
                break;
            
            case "0038":
                $rule_revision = '00';
                if( in_array( $blz, array("26691213", "28591579", "25090300") ) )
                        $blz = "28590075";
                break;
            
            case "0039":
                $rule_revision = '00';
                if( in_array( $blz, array("25621327", "26520017", "26521703",
                    "26522319", "26620010","26621413","26720028","28021002",
                    "28021301", "28021504","28021623","28021705","28021906",
                    "28022015", "28022412","28022511","28022620","28022822",
                    "28023224", "28023325","28220026","28222208","28222621",
                    "28320014", "28321816","28420007","28421030","28520009",
                    "28521518", "29121731") ) )
                        $blz = "28020050";
                break;
            
            case "0040":
                $rule_revision = '00';
                $blz = "68052328";
                break;
            
            case "0041":
                $rule_revision = '00';
                if( $blz == "62220000" ) {
                    $blz = "50060400";
                    $account = "11404";
                }
                break;
            
            case "0042":
                if( strlen( $account ) != 8 ) return false;
                if( !((int)$account >= 50452000 && (int)$account <= 50453999
                        || (int)$account >= 50459000 && (int)$account <= 50459999
                        || $account{3} == "0")) return false;
                if( (int)substr($account,-5) >= 0 && (int)substr($account,-5) <= 999 )
                        return false;
                break;
            
            case "0043":
                $rule_revision = '00';
                if( $blz == "60651070" ) $blz = "66650085";
                break;
            
            case "0044":
                $rule_revision = '00';
                if( $account == "202" ) $account = "2282022";
                break;
            
            case "0045":
                $rule_revision = '00';
                /**
                 * Einer nach der Standard IBAN-Regel ermittelten IBAN ist
                 * stets der BIC ESSEDE5FXXX zuzuordnen
                 */
                break;
            
            case "0046":
                $rule_revision = '00';
                $blz = "31010833";
                break;
            
            case "0047":
                $rule_revision = '00';
                $account = str_pad( $account, 10, '0' );
                break;
            
            case "0001":
            default:
                return false;
        }
        
        if( substr( $rule, -2 ) !== $rule_revision ) {
            $blz = $blz_backup;
            $account = $account_backup;
            throw new exception( "The implemented rule is outdated." );
        }
        
        return true;
        
    }
    
    /**
     * @return bool returns TRUE if it is allowed to generate an IBAN at all
     */
    /*function prepareParametersForIBAN_Revert( &$account, &$blz ) {
        
        $rule = $this->chooseRule($blz);
        
        switch( $rule ) {
                
            case "000000":
                break;
            
            case "001100":
                if( in_array( $account,  array( "8010001", "47803" ) ) ) return( false );
                break;
            
            case "003700":
                if( $blz == "30010700" ) return( false );
                break;
            
            case "004000":
                $blz = "68052328";
                break;
            
            default:
            case "000100":
                throw new InvalidArgumentException("There shouldn't be an IBAN with this german bankcode.");
        }
        
        return true;
        
    }*/
        
    public function genIBAN( &$attributes ) {
        
        $account = $attributes[IBAN::ACCOUNT];
        $blz = $attributes[IBAN::NATIONALBANKCODE];
        
        $proceed = $this->_prepareParametersForIBAN($account, $blz);
        
        // Check, whether it's allowed to proceed
        if( $proceed !== TRUE ) {
            throw new exception("It's not allowed to create an IBAN in this case!");
        }
        
        $payload = sprintf( '%s%s%s00',
                $blz,
                str_pad( $account, 10, '0', STR_PAD_LEFT ),
                $attributes[IBAN::COUNTRY],
                '00');

        $attributes[IBAN::CHECKSUM] = iso7064_mod_97_10( $payload );
        
        $return = sprintf( '%s%s%s%s', $attributes[IBAN::COUNTRY],
                $attributes[IBAN::CHECKSUM], $blz,
                str_pad( $account, 10, '0', STR_PAD_LEFT ) );
        
        return( $return );
        
    }
    
    /**
     * 
     * @param string $IBAN
     * @return bool
     * @todo not developed yet
     * This function generates the IBAN using an existing IBAN's fragments. If
     * the generated IBAN results in the same value as the passed one, it is 
     * according the specific rules.
     */
    public function accordingNationalRules( $IBAN ) {
        
        try{
        
            $account = $this->_extractAccount($IBAN);
            $bankcode = $this->_extractBankcode($IBAN);
            $country = substr( $IBAN, 0, 2 );
            
            $attributes = array(
                IBAN::ACCOUNT => $account,
                IBAN::NATIONALBANKCODE => $bankcode,
                IBAN::COUNTRY => $country
            );
                    
            $newIBAN = $this->genIBAN( $attributes );
            
            return( $newIBAN == $IBAN );
            
        }
        catch( Exception $e ) {
            return( FALSE );
        }
        
        return( TRUE );
    }
    
    /**
     * This function extracts the BLZ part of the IBAN. It expects the IBAN to
     * be valid. There won't be any check within this method
     * 
     * @param $IBAN
     * @return string
     */
    private function _extractBankcode( $IBAN ) {
        
        // check the bankcode fragment
        $tmpBankcode = substr( $IBAN, 4, 8 );
        $tmpAccount = substr( $IBAN, 12, 10 );
        
        // choose the rule for the bankcode fragment ...
        $rule = $this->_chooseRule( $tmpBankcode );
        
        // ... if it didn't throw an exception, than check, whether the rule
        // of the set of bankcode and account was allowed to be created as an
        // IBAN. First copy the bankcode, to check, whether the rule allows this
        // bankcode to be used.
        $copyBankcode = $tmpBankcode;
        $allowed = $this->_prepareParametersForIBAN( $tmpAccount, $copyBankcode );
        
        if( !$allowed || $copyBankcode !== $tmpBankcode )
            throw new exception("This IBAN is invalid!");
        
        return( $tmpBankcode );
        
        // check if there is a special rule for the particular bankcode fragment and, if appliable, apply it to the
        // tmpBankcode parameter
        /*if( $this->prepareParametersForIBAN_Revert( $tmpAccount, $tmpBankcode ) ) {
        
            // if the bankcode can be 
            return( $tmpBankcode );
        }
        
        else return( "" );*/
    }
    
    private function _extractAccount( $IBAN ) {
        
        $tmpBankcode = substr( $IBAN, 4, 8 );
        $tmpAccount = (int) substr( $IBAN, 12, 10 );
        
        // choose the rule for the bankcode fragment ...
        $rule = $this->_chooseRule( $tmpBankcode );
        
        // ... if it didn't throw an exception, than check, whether the rule
        // of the set of bankcode and account was allowed to be created as an
        // IBAN. First copy the account to check, whether the rule allows the
        // account to be used
        $copyAccount = $tmpAccount;
        $allowed = $this->_prepareParametersForIBAN( $copyAccount, $tmpBankcode );
        
        if( !$allowed || $copyAccount !== $tmpAccount )
            throw new exception("This IBAN is invalid!");
        
        return( $tmpAccount );
        
        /*if( $this->prepareParametersForIBAN_Revert( $tmpAccount, $tmpBankcode ) ) {
            return( $tmpAccount );
        }
        
        return "";*/
                
    }
    
    public function extractInformation( $IBAN, $attrib ) {
        
        switch( $attrib ) {
            case IBAN::NATIONALBANKCODE:
            return( $this->_extractBankcode($IBAN) );
                
            case IBAN::ACCOUNT:
            return( $this->_extractAccount($IBAN) );
        }
        
    }
    
}

?>
