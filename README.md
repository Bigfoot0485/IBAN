IBAN
====
Generates and validates IBANs and BIC according to the international standards
and according to the special rules, which apply in some countries like Germany.

Release Notes
------------------------
0.0.1  Initial release

Roadmap
------------------------
1.0.0  Validates all IBANs, incl. all special rules for IBANs of Germany according to the publicated rules from July 2013
1.0.1  Update of the German rules to match the latest ones
1.1.0  Returns the BIC of German finance institutes
1.2.0  Implements a cache, which speeds up the process especially for batch processes
1.2.1  Allow formated IBANs to be passed and to be return
1.3.0  Returns details of the finance institute, like Name or PLZ
1.3.1  Add a validation for the account's checksum, which should also be part of the standard rule
1.3.2  Add PHPDoc for easier development
1.4.0  Implements detailled logging states

License
------------------------
Copyright (c) 2013 Sven Lötgering
This library is been distributed with a MIT license.
For details, please refer the file /LICENSE

The autor
------------------------
Sven Lötgering is developing web- and db-based applications since 2001. First
in touch with ASP, he is now focused on PHP5, PERL, PLSQL, Linux, Oracle and
MySQL administration. Currently he is working as a projectmanager doing software
projects for public authorities.

Get in contact with Sven at php-dev@sven-loetgering.de