var tail = require("../tailnative");
var http = require("http");

var filename = process.argv[2];

if (!filename)
    return sys.puts("Usage: node http-realtime-example.js filename");

var t = tail.start(filename);

http.createServer(function(req,res){
    res.writeHead(200,{"Content-Type": "text/plain"});
    t.on('data', function (data) {
        console.log(data);
        res.write(data + "\r\n");
    });  
}).listen(8000);

