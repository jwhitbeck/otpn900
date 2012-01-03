OTP Generator for N900
======================

Description
-----------

One-time password generator for the Nokia N900. Supports OAUTH tokens
(HOTP, TOTP) as well as the Mobile-OTP algorithm.

Building
--------

If you wish to rebuild the package from source, then you will need a
functioning [Maemo 5
SDK](http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide). Then,
at the root of the project, run the following commands:

    $ autoreconf --force --install
    $ ./configure
    $ make

Installation
------------

As of Aug. 16, 2011, this package is in the extras-devel Maemo
repository. It is being promoted to extras-testing. For information on
how to activate the 'Extras' repositories on your device refer to
[http://wiki.maemo.org/Extras](http://wiki.maemo.org/Extras).

Once the extras repositories have been enabled, install the 'otp'
package, either through the 'App Manager' or by running (as root):

    # apt-get install otp


Alternatively, you may download the binary package
(otp\_0.1-1\_armel.deb) from either the [Maemo
repository](http://repository.maemo.org/extras-devel/pool/fremantle/free/o/otp/otp_0.1-1_armel.deb)
or from
[neush.net](http://neush.net/static/otpn900/otp_0.1-1_armel.deb). Copy
it to your N900 phone, and, run (as root):

    # dpkg -i otp_0.1-1_armel.deb


Google 2-step Authentication Support
------------------------------------

Google uses Base32 keys for their 2-step authentication. The OTPN900
maemo program cannot use these directly, but will work fine if the
Base32 key is first converted to hexadecimal before being entered into
OTPN900. For a detailed tutorial, refer to the discussion
[here](http://github.com/neush/otpn900/issues/1). Many thanks to Yao
Wei for his help with this.


See also
--------

* RFC4226 for the HOTP and TOTP standard
* [motp.sf.net](http://motp.sourceforge.net) for a description of the MOTP algorithm

Screenshots
-----------

![Profile selection](http://neush.net/static/otpn900/screen1.jpg "Profile selection")
![HOTP](http://neush.net/static/otpn900/screen2.jpg "HOTP")
![HOTP Config](http://neush.net/static/otpn900/screen3.jpg "HOTP configuration")
![Pin pad](http://neush.net/static/otpn900/screen4.jpg "Pin code entry for MOTP")
![MOTP](http://neush.net/static/otpn900/screen5.jpg "MOTP")
