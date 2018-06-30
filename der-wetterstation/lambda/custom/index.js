exports.handler = function( event, context ) {

    var http = require( 'http' );

    var options = {
        host: 'esesmarthome.azurewebsites.net',
        path: '/api/alexa/measurements/' + event.request.intent.slots.deviceID.value,
        method: 'GET',
        headers: {
            'Content-Type': 'application/json'
        }
    };

    var req = http.request(options, function (res) {
        var chunks = [];

        res.on("data", function (chunk) {
            chunks.push(chunk);
        });

        res.on("end", function () {
            var body = Buffer.concat(chunks);
            var data = JSON.parse(body);
            var temperatur = data.temperature + '';
            temperatur = temperatur.replace('.',',');
            var text = 'Vielen Dank, dass du das beste Projekt nutzt. Die Temperatur in deinem ' + event.request.intent.slots.deviceID.value+ ' beträgt ' + temperatur + ' Grad Celsius' + ', und die Luftfeuchtigkeit beträgt ' + data.humidity + ' Prozent';
            output(text,context);
        });
    });

    req.end();
};

function output( text, context ) {

    var response = {
        outputSpeech: {
            type: "PlainText",
            text: text
        },
        shouldEndSession: true
    };

    context.succeed( { response: response } );

}