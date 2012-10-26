Native module for node.js .

Read new line in real time, as 'tail - f'.

Install:
========

	npm install tailnative

or

	git clone git://github.com/whitesheep/node-tail-native.git
	cd node-tail-native
	node-gyp configure build


Use: 
========

Tailnative, from 0.3 version, support `line separator`
	
	new Tail(filename, "\r\n");



_Event emits:_

data 	`function(data){}`
end		`function(){}`
error	`function(error){}`



_Example code :_

	var Tail = require("tailnative");
	
	var lineseparator = "\n";
	var tail = new Tail(__dirname + "/testfile", lineseparator);
	
    
	tail.on('data', function(data){
	    console.log(this.file + ': ' + data);
	});

	tail.on('error', function(){
	    console.log('error');
		tail.close();
	});

	tail.on('end', function(){
	    console.log('end');
	});
