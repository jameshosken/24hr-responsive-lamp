
let res;
let waiting = false;

let currentMode = "";

let sendFunctions = [];

let buttons = [];
let buttonLabels = [
	"Candle Mode",
	"Timing Mode"]

let url = 'http://192.168.1.9/';

function setup(){
	createCanvas(window.innerWidth, window.innerHeight);
 	 
 	sendFunctions = [sendAuto, sendTiming];

 	console.log(sendFunctions);


	for(let i = 0; i < sendFunctions.length; i++){
		console.log(i);
		console.log(sendFunctions[i])
		let button = createButton(buttonLabels[i]);
		button.position(20, 20 + i*25);
		button.mousePressed(sendFunctions[i]);
		buttons.push(button);
	}
	
}


function draw() {
	background(0);
	fill(255);
	text(currentMode, 50, 50);
}


let sendAuto = function(){
	sendMessage('A');
}

let sendTiming = function(){
	sendMessage('T');
}




function sendMessage(value){ 

	if(!waiting){
		waiting = true;

		console.log("KEY: " + value);
		let msg = url+"?"+value;
		console.log("Sending: " + msg);
		try{
			httpDo(
			    msg,
			    {
			      method: 'GET',
			      // Other Request options, like special headers for apis
			      headers: { 'Access-Control-Allow-Origin': '*' }
			    },
			    function(res) {
			      	currentMode = value;
			    	res = response;
			    	console.log(res);
			    	waiting = false;
			    },
			    function(err) {
			      	console.log("RES ERR");
			      	console.log(err);
			      	waiting = false;
			    }
			);
		}catch(e){
			console.log("ERROR:")
			console.log(e);
		}
	}

}


