exports.handler = function( event, context ) { //Handlerfunktion für Alexa Aufruf

    var http = require( 'http' ); //Binde HTTP Client ein

    // Informationen über Backend 
    // Rufe den GET /api/alexa/mmeasurements/{Gerätename} endpoint auf 
    // welcher Temperatur und Luftfeuchtigkeit zurückliefert 
    // Gerätename wird über Intent-Slot von Alexa übergeben
    var options = {
        host: 'esesmarthome.azurewebsites.net', 
        path: '/api/alexa/measurements/' + event.request.intent.slots.deviceID.value,
        method: 'GET', 
        headers: {
            'Content-Type': 'application/json'
        }
    };
// HTTP CALL
    var req = http.request(options, function (res) {
        var chunks = [];

        res.on("data", function (chunk) {
            chunks.push(chunk);
        });

        //Verareite Response
        res.on("end", function () {
            var body = Buffer.concat(chunks);
            var data = JSON.parse(body); //Parse JSON Objekt
            var temperatur = data.temperature + '';
            temperatur = temperatur.replace('.',','); // Ersetze . furch , da Alexa sonst die Temperatur falsch ausspricht
            // Zusammnestöpseln des Ausgabetexts
            var text = 'Vielen Dank, dass du das beste Projekt nutzt. Die Temperatur in deinem ' + event.request.intent.slots.deviceID.value+ ' beträgt ' + temperatur + ' Grad Celsius' + ', und die Luftfeuchtigkeit beträgt ' + data.humidity + ' Prozent';
            output(text,context); // Übergeb Antwort
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