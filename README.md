# webserv

## How to config

The default root is the top directory of the webserv.  
Webserv will search for the called config file in *~/var/sites_enabled/*  
So the default config is ***~/var/sites_enabled/config.webserv***
an example config file can look like this:  

    server {

        listen 80;
  
        serverName localhost www.localhost;
  
        errorPage 404 /error.html;
  
        return 302 http://localhost:8200$uri;
  
        location /example_directory/ {
            root    /var/www/html;
            index  file.html;
        }

        location /example_file {
            root	/var/www/html;
            autoindex on;
        }

        location /cgt_executable {
            root	/var/cgi-bin;
            cgi_param /ret_time.py;
        }

        location \.py$ {
            root /var/www/html;
            cgi_param /cgi-dir/;
            autoindexing on;
        }
    }

* The standard port where a server will listen to is 80
* Within errorPage you can define custom error pages formatted as: 
> 'nbr'-'space'-'file'-'space'-...;
* If a serverblock should redirect the return statement can be used as follows:
> 'nbr'-'space'-'destination''$uri';  

for the location blocks there are some rules as well:
  * Index will define the index for a location
  * if the specified location ends with a **'/'** it will be handles like a directory
    > example of a directory: ***location /example_directory/*** will be a directory  
     ***location /example_file*** will be a file
  * the root will always be handled like a directory
  * if the cgi_param is defined it will be handled by cgi
  * the default root for cgi programs is ~/var/cgi-bin
  * only python scripts will be handled by CGI
  * by specifying a **\\.py$** location block, all .py files 
    in the specified root/cgi_param/ dir en sub dir's will be handled by CGI

## How to run
```bash
cmake .
make webserv
./webserv [configuration file]
```

open your browser and head to localhost: and than the portnbr specified in the config file

inspect > network > enter localhost:80 in address bar  
to see response message details
