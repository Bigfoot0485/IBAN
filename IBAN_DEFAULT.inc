<?php
/**
 * Description of IBAN_DEFAULT
 *
 * @author sven
 */
require_once("IBAN.inc");
require_once("fxISO7064_mod_97_10.php");

class IBAN_DEFAULT {
    
    private $maxLength = 34;
    private $_regexp = "/^[A-Z]{2}\d{2}[A-Z0-9]{12,30}$/";
    private $country;
    
    function __construct( $COUNTRY ) {
        
        if( in_array( $COUNTRY, array("GR") ) ) {
            $this->country = $COUNTRY;
            $this->maxLength = 27;
        }
        
        if( in_array( $COUNTRY, array("BH", "BG", "GE", "DE", "IE", "ME", "RS", "GK") ) ) {
            $this->country = $COUNTRY;
            $this->maxLength = 22;
        }
        
        if( in_array( $COUNTRY, array("NL", "GL", "FI", "FO", "DK") ) ) {
            $this->country = $COUNTRY;
            $this->maxLength = 18;
        }
        
    }
    
    function isLengthRight( $IBAN ) {
        
        if( length( $IBAN ) == $this->maxLength ) {
            return( TRUE );
        }
        
    }
    
    function areAllAttribsSet( $keys ) {
        
        switch( $this->country ) {
            case "DE":
                return( in_array( IBAN::ACCOUNT, $keys ) && in_array( IBAN::NATIONALBANKCODE, $keys ) );
                break;
        }
        
        return( FALSE );
        
    }
    
    public function genIBAN( &$attributes ) {
        
        $payload = $attributes[IBAN::NATIONALBANKCODE] 
                        . $attributes[IBAN::ACCOUNT] 
                        . $attributes[IBAN::COUNTRY] . '00';
        
        $attributes[IBAN::CHECKSUM] = iso7064_mod_97_10( $payload );
        
        return( $attributes[IBAN::COUNTRY] 
                . $attributes[IBAN::CHECKSUM]
                . $attributes[IBAN::NATIONALBANKCODE] 
                . $attributes[IBAN::ACCOUNT] );
        
    }
    
    /**
     * 
     * @param $IBAN
     * @return bool
     * @final
     */
    public function matchesExpression( $IBAN ) {
        
        $matchesExpression = (preg_match( $this->_regexp, $IBAN )===1);
        
        return( $matchesExpression );
        
    }
    
    /**
     * Countries, who have their own rules, have to overload this method
     * Every other country is basically TRUE, as there is no national specific
     * rule
     * @param string $IBAN
     * @return bool
     */
    public function accordingNationalRules( $IBAN ) {
        return( TRUE );
    }
    
    public function extractInformation( $IBAN, $attrib ) {
        return;
    }
    
    /**
     * 
     * @param type $IBAN
     * @return type
     * @link http://de.wikipedia.org/wiki/International_Bank_Account_Number#Validierung_der_Pr.C3.BCfsumme Validating the IBAN reference in Wikipedia
     */
    public function isCheckSumCorrect( $IBAN ) {
        
        // First put the first four letters to the end
        $input = substr($IBAN, 4) . substr($IBAN, 0, 4);
        $outsput = iso7064_mod_97_10($input);
        return( (int)$outsput % 97 == 0 );
        
        // replace capital letters by numbers from 10 to 35
        /*for($i=10;$i<=35;$i++) {
            $input = str_replace( chr(55+$i), $i, $input );
        }*/
        
    }
    
}

?>
