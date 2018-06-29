exports.handler = function( event, context ) {

    var http = require( 'http' );

    var post_options = {
        host: 'esesmarthome.azurewebsites.net',
        path: '/api/alexa/measurements',
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        }
    };

    var req = http.request(post_options, function (res) {
        var chunks = [];

        res.on("data", function (chunk) {
            chunks.push(chunk);
        });

        res.on("end", function () {
            var body = Buffer.concat(chunks);
            var data = JSON.parse(body);
            var text = 'Vielen Dank, dass du das beste Projekt nutzt. Die Temperatur in deinem Wohnzimmer beträgt ' + data.temperature + ' Grad Celsius' + ', und die Luftfeuchtigkeit beträgt ' + data.humidity + ' Prozent';
            output(text,context);
        });
    });

    req.write(JSON.stringify(1));
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