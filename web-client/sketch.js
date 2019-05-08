
let res;
let waiting = false;

let currentMode = "";

let url = 'http://192.168.1.9/';
function setup(){
}


function draw() {
	background(0);
	fill(255);
	text(currentMode, 50, 50);
}

function keyPressed(){ 

	if(!waiting){
		waiting = true;
		let val = key;
		console.log("KEY: " + val);
		let msg = url+"?"+val;
		console.log("Sending: " + msg);

		httpDo(
		    msg,
		    {
		      method: 'GET',
		      // Other Request options, like special headers for apis
		      headers: { 'Access-Control-Allow-Origin': '*' }
		    },
		    function(res) {
		      	currentMode = val;
		    	res = response;
		    	console.log(res);
		    	waiting = false;
		    }
		);
	}

}


// httpGet(msg, 'jsonp',  function(response) {
// 	currentMode = val;
//    	res = response;
//    	console.log(res);
//    	waiting = false;
//  	});