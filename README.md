# smart-home-automation
 validating my 3d printer purchase

## Organization of code:

### src
contains arduino code

### web
contains web server (framework undecided as of yet) as well as ESB (apache camel routing system)


## Flow 1

On startiup, arduino sends a request to running web server, message contains its ip address. This address is logged and stored (registered) and set as an endpoint

Arduinos should have 2 endpoints: their assigned purpose (eg. open/close blind) and a health check, to be run every 5 minutes.

This data will be stored in a


TODOS:
- Arduino Hardware stuff (Buttons, LEDs, Motor, etc)

docker container name: sql1