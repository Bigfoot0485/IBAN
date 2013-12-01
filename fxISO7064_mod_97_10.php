<?php
/**
 * 
 * @param string $input
 * @return string
 * @assert (428600030015246900DE00 ) == 42
 */
/*function iso7064_mod_97_10 ( $input ) {
    
    // replace capital letters by numbers from 10 to 35
    for($i=10;$i<=35;$i++) {
        $input = str_replace( chr(55+$i), $i, $input );
    }
    
    // calc the rest of a devision by 97 and subtrate that result from 98
    $return = 98 - ( (int) $input % 97 );
    
    // fill up the result with a zero, if lower than 10
    return( str_pad( $return, 2, '0', STR_PAD_LEFT ) ) ;
    
}*/
function iso7064_mod_97_10 ( $input ) {
    
    // replace capital letters by numbers from 10 to 35
    for($i=10;$i<=35;$i++) {
        $input = str_replace( chr(55+$i), $i, $input );
    }
    
    // calc the rest of a devision by 97 and subtrate that result from 98
    // $return = 98 - ( (int) $input % 97 );
    // $return = 98 - ( (int) bcmod( $input, 97 ) );
    $rest = substr( $input, 0, 2 );
    for( $i = 2; $i<strlen( $input ); $i+=7 ) {
        $rest = $rest . substr( $input, $i, 7 );
        $rest = $rest % 97;
    }
    
    $return = 98 - $rest;
    
    // fill up the result with a zero, if lower than 10
    return( str_pad( $return, 2, '0', STR_PAD_LEFT ) ) ;
    
    
}
?>
