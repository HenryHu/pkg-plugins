## General Information

The *pkg-plugin-compare* plugin is used for comparing information of installed packages and remote packages.
Currently it compares:

* version
* options

## How to build the plugin?

In order to build the plugin enter into the plugin's directory and run make(1), e.g.:

	$ cd /path/to/pkg-plugins-compare
	$ make
	
Once the plugin is built you can install it using the following command:

	$ make install 
	
The plugin will be installed as a shared library in ${PREFIX}/lib/pkg/compare.so

## Testing the plugin

To test the plugin, first check that it is recongnized and
loaded by pkgng by executing the `pkg plugins` command:

	$ pkg plugins
	NAME       DESC                                       VERSION
	compare    Plugin for comparing local and remote pkgs 1.0.0

If the plugin shows up correctly then you are good to go!

Use

    $ pkg compare <pkg name>

to compare the installed version with the repo version.

