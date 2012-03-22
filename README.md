Native module for node.js .

Read new line in real time, as 'tail - f'.

Install:
========

	npm install tailnative



Example: 
========

	var tail = require('tailnative');
	
	tail.start(__dirname + '/testfile', function(t){
	    
	    t.on('data', function(data){
	        console.log(t.file + ': ' + data);
	    });
	    
	    t.on('error', function(){
	        console.log('error');
	    });
	    
	    t.on('end', function(){
	        console.log('end');
	    });
	    
	    setTimeout(function(){
	        t.stop();
	    }, 6000);
	});
