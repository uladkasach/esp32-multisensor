var PORT = 3000;
var HOST = '192.168.43.48';

var dgram = require('dgram');
var server = dgram.createSocket('udp4');
var fs = require('fs');


/*
    parsing methods
*/
function parse_and_record_message(message){
    //console.log("message : ");
    //console.log(message);
    var frames = message.split("#");
    frames.forEach((frame)=>parse_and_record_frame(frame));
}
function parse_and_record_frame(frame){
    var sections = frame.split(";");
    if(sections.length < 2) return; // not a real accustic frame, probably a trash message
    sections = sections.slice(0, -1);
    var csv_string = sections.join(",") + "\n";
    fs.appendFileSync('logs.csv', csv_string);
    //console.log(csv_string);
}
/*
    // testing utils
    var message = "12:56:11;00001567;00000899;00000814;00005225;#12:56:11;00001586;00001296;00000814;00005965;#12:56:11;00001551;00001246;00000808;00005736;#12:56:11;00001565;00000899;00000811;00005865;#12:56:11;00001582;00001248;00000811;00005663;#12:56:11;00001554;00001246;00000810;00005813;#12:56:11;00001565;00000924;00000808;00005885;#12:56:11;00001552;00000899;00000809;00005912;#12:56:11;00001583;00001247;00000812;00005764;#12:56:11;00001565;00001246;00000809;00005762;#���?x�";
    parse_and_record_message(message);
*/


/*
    start udp server
*/
server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (message, remote) {
    console.log(remote.address + ':' + remote.port +' - ' + message);
    parse_and_record_message(message.toString('utf8'));
});
server.bind(PORT, HOST);
