<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
require_once("IBAN.inc");

// Erstelle gültige IBAN anhand einer IBAN und gebe das Land aus
$konto = new IBAN ( "DE42428600030015246900" );
printf("IBAN: \"%s\"", $konto->getIBAN()) ;
printf("Land: \"%s\"", $konto->getAttribute( IBAN::COUNTRY ) ) ;

// Erstelle gültige IBAN anhand deutscher Bankdaten
$konto2 = new IBAN();
$konto2->setAttribute(IBAN::COUNTRY, "DE");
$konto2->setAttribute(IBAN::NATIONALBANKCODE, "42860003");
$konto2->setAttribute(IBAN::ACCOUNT, "15246900");
printf("IBAN: \"%s\"", $konto2->getIBAN()) ;
printf("Land: \"%s\"", $konto2->getAttribute( IBAN::COUNTRY ) ) ;
?>
