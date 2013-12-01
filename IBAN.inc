<?php

/**
 * Description of IBAN
 *
 * @author sven
 */
require_once( "fxISO7064_mod_97_10.php");

class IBAN {
    
    // Makes all IBAN based attributes of an account available to the calling
    // script
    const IBAN = 1;
    const COUNTRY = 2;
    const CHECKSUM = 3;
    const NATIONALBANKCODE = 4;
    const ACCOUNT = 5;
    
    /**
     *
     * @TODO complete IBAN COUNTRIES
     */
    private $_IBAN_COUNTRIES = array("DE","NL","GR","GB","SA","CH","IL");
    private $_ATTRIBUTES = array();
    private $_VALIDATOR;
    
    private function _isValid ( $IBAN ) {
        
        // Check, whether the country is valid
        if( in_array( substr( $IBAN, 0, 2 ), $this->_IBAN_COUNTRIES ) ) {
            
            // Initiate the validator for the particular country
            $VALIDATOR = $this->_chooseValidator( substr( $IBAN, 0, 2 ) );
            
            // check, whether the IBAN "looks" correctly
            if( $VALIDATOR->matchesExpression( $IBAN ) ) {
                
                // check, whether the checksum is right
                if( $VALIDATOR->isCheckSumCorrect( $IBAN ) ) {
                
                    // check, whether the IBAN is according the national rules
                    if( $VALIDATOR->accordingNationalRules( $IBAN ) ) {

                        return (TRUE );

                    }
                
                }
                
            }
            
        }
        
        return FALSE;
        
    }
    
    private function _chooseValidator( $country ) {
        
        $VALIDATOR = null;
        
        switch( $country ) {
                    
            case "DE":
                require_once("IBAN_DE.inc");
                $VALIDATOR = new IBAN_DE();
                break;

            default:
                require_once("IBAN_DEFAULT.inc");
                $VALIDATOR = new IBAN_DEFAULT( $country );
                break;

        }
            
        return( $VALIDATOR );
        
    }
    
    /**
     * The constructor allows to pass a ready IBAN.
     * @param string $IBAN
     * @return IBAN
     */
    public function __construct( $IBAN = null ) {
        
        if( !is_null( $IBAN ) ) {
            
            $this->setIBAN( $IBAN );
            
        }
        
    }
    
    /*public function getBIC( ) {
        
    }*/
    
    /**
     * Returns the attribute, which stores
     * @return string
     * @throws InvalidArgumentException
     */
    public function getIBAN( ) {
        
        return $this->getAttribute( self::IBAN );
        
    }
    
    public function setIBAN( $IBAN ) {
        
        // If I set the IBAN this way, I need to reset the other attributes
        unset( $this->_ATTRIBUTES[self::NATIONALBANKCODE] );
        unset( $this->_ATTRIBUTES[self::ACCOUNT] );
        unset( $this->_ATTRIBUTES[self::COUNTRY] );
        unset( $this->_ATTRIBUTES[self::IBAN] );
        
        // First check if the IBAN is valid, before the routine is allowed to
        // process it
        if ( $this->_isValid( $IBAN ) ) {
            
            $this->_VALIDATOR = $this->_chooseValidator(  substr( $IBAN, 0, 2 ) );
            $this->_ATTRIBUTES[IBAN::NATIONALBANKCODE] = $this->_VALIDATOR->extractInformation( $IBAN, IBAN::NATIONALBANKCODE );
            $this->_ATTRIBUTES[IBAN::ACCOUNT] = $this->_VALIDATOR->extractInformation( $IBAN, IBAN::ACCOUNT );
            $this->_ATTRIBUTES[IBAN::COUNTRY] = substr( $IBAN, 0, 2 );
            $this->_ATTRIBUTES[IBAN::IBAN] = $IBAN;
            
        } 
        
    }
    
    public function getAttribute( $attrib ) {
        
        if( in_array( $attrib, array( self::NATIONALBANKCODE, self::IBAN, self::CHECKSUM, self::ACCOUNT, self::COUNTRY ) ) ) {
            
            if( in_array( $attrib, array_keys( $this->_ATTRIBUTES ) ) ) {
                
                return( $this->_ATTRIBUTES[$attrib] );
                
            } else {
                
                throw new InvalidArgumentException("This information is not yet known.");
                
            }
            
        } else {
            
            throw new InvalidArgumentException("You have passed an invalid attribute!");
            
        }
        
    }
    
    
    /**
     * @return boolean
     * @todo It's not been implemented yet
     * Return whether an IBAN, which has been passed by setIBAN, the constructor
     * or is been calculated by the class itself is valid. The last mentioned
     * case should always be true, if the class is doing it's job correctly.
     * 
     * If it is not valid, than it is either not valid, or it is not been
     * checked to be valid, so in case of FALSE, the validation process has
     * to be fired first
     */
    public function isValid( ) {
         
        // If the user passed an IBAN, use that one for validation
        if( isset( $this->_ATTRIBUTES[IBAN::IBAN] ) ) {
            return( $this->_isValid( $this->_ATTRIBUTES[IBAN::IBAN] ) );
        }
         
        // If the user passed the other informations, for the IBAN creation
        // use that one to determine, whether it is valid or not
        /*if( !is_null( $this->_VALIDATOR ) ) {
            
            if( $this->_VALIDATOR->areAllAttribsSet( array_keys( $this->_ATTRIBUTES ) ) ) {
                
                $iban = $this->_VALIDATOR->genIBAN( $this->_ATTRIBUTES );
                return( $this->_isValid( $iban ) );
                
            }
            
        }*/
        
        else return FALSE;
        
    }


    public function setAttribute( $attrib, $value ) {
        
        // Ensure, that the user won't set the checksum
        if( $attrib == self::CHECKSUM ) {
            throw new InvalidArgumentException("The checksum is a readonly attribute!");
        }
        
        // The Iban can be set on this way, but should be done with setIban
        if( $attrib == self::IBAN ) {
            $this->setIBAN( $value );
            return;
        }
        
        if( $attrib == self::COUNTRY ) {
            
            if( in_array( $value, $this->_IBAN_COUNTRIES ) ) {
                
                $this->_VALIDATOR = $this->_chooseValidator( $value );
                        
            }
            
            else { throw new InvalidArgumentException("An invalid country has been passed!", 10 ) ; }
            
        }
        
        if( in_array( $attrib, array( self::COUNTRY, self::ACCOUNT, self::NATIONALBANKCODE))) {
            $this->_ATTRIBUTES[$attrib] = $value;
            unset( $this->_ATTRIBUTES[self::IBAN] );
        }
        
        // If all informations are available, the IBAN can be generated
        if( isset( $this->_VALIDATOR ) ) {
            
            if( $this->_VALIDATOR->areAllAttribsSet( array_keys( $this->_ATTRIBUTES ) ) ) {
                
                $this->_ATTRIBUTES[self::IBAN] = $this->_VALIDATOR->genIBAN( $this->_ATTRIBUTES );
                
            }
            
        }
        
    }
    
}

?>
