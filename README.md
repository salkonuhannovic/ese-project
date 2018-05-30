# Der-Wetterstation

Der-Wetterstation is a smart home assistant which provides you information about temperature and humidity. This information is accessible via a web-fronted and also via Alexa-Skill.

Used technologies are :
  - Tivia C Launchboard with HTU-21 click board for measuring the temperature & humidity
  - Amazon ASK CLI with node.js for providing the Alexa Skill with corresponding Lambda function
  - .NET for Webfrontend & REST-API


# How does it work?
The µC measures the temperature and the humidity which afterwards is getting post to a database in a parameterized intervall. The data post happens via a REST call from the µC to the provided backend. The data is getting processed by the backend. The frontend application shows the data which is stored in the database. The Alexa skill is accesing the data by REST-API which is also provided by the backend.

The smarthome assistent consists of three packages(folders within this project):
- der-wetterstation: code of the alexa skill 
- tivia-c: code of the µC
- frontend&backend: code of frontend and backend


### Tech
Der-Wetterstation uses a number of open source projects to work properly:

* [Jsmen] - awesome JSON parser for C
* [Twitter Bootstrap] - great UI boilerplate for modern web apps
* [node.js] - for Amazon Lambda function
* [Gulp] - the streaming build system
* [jQuery] - duh

And of course The-Wetterstation itself is open source on GitHub.

### Todos

 - finish the prototype :D

   [Jsmen]: <https://github.com/zserge/jsmn>
   [node.js]: <http://nodejs.org>
   [Twitter Bootstrap]: <http://twitter.github.com/bootstrap/>
   [jQuery]: <http://jquery.com>
   [AngularJS]: <http://angularjs.org>
   [Gulp]: <http://gulpjs.com>

