# webserv

## How to config

The default root is the top directory of the webserv.  
Webserv will search for the called config file in *~/var/sites_enabled/*  
So the default config is ***~/var/sites_enabled/config.webserv***
an example config file can look like this:  

    server {

      listen 80;
  
      server_name localhost www.localhost;
  
      index   index.html index.htm;
  
      error_page 404 /error.html;
  
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
    }

* The standard port where a server will listen to is 80
* Index will define the index if none is specified for a directory
* Within error_page you can define custom error pages formatted as: 
> 'nbr'-'space'-'file'-'space'-...;
* If a serverblock should redirect the return statement can be used as follows:
> 'nbr'-'space'-'destination''$uri';  

for the location blocks there are some rules as well:
  * if the specified location ends with a **'/'** it will be handles like a directory
    > example of a directory: ***location /example_directory/*** will be a directory  
     ***location /example_file*** will be a file  
  * if it ends without a '/' it will be handles as a file
  * the root will always be handles like a directory
  * if the cgi_param is defined it will be handles by cgi
  * the default root for cgi programs is ~/var/cgi-bin
  * only python scripts will be handled by CGI

## How to run
```bash
cmake .
make webserv
./webserv [configuration file]
```

open your browser and head to localhost: and than the portnbr specified in the config file

inspect > network > enter localhost:80 in address bar  
to see response message details
