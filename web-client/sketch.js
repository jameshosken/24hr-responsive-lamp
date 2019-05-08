
let res;
let waiting = false;

let currentMode = "";

let sendFunctions = [];

let buttons = [];
let buttonLabels = [
	"Timing Mode",
	"Candle Mode",
	"Reading Mode",
	"Sunrise Mode",
	"Sunset Mode",
	"Demo Mode"]

let url = 'http://';

let ip = "";
let ipInput;

function setup(){

	ipInput = createInput("IP ADDRESS");
	ipInput.position(20, 20);

	createCanvas(window.innerWidth, window.innerHeight);
 	 
 	sendFunctions = [sendTiming, sendCandle, sendReading, sendSunrise, sendSunset, sendDemo];

 	console.log(sendFunctions);


 	 

	for(let i = 0; i < sendFunctions.length; i++){
		console.log(i);
		console.log(sendFunctions[i])
		let button = createButton(buttonLabels[i]);
		button.position(20, 50 + i*25);
		button.mousePressed(sendFunctions[i]);
		buttons.push(button);
	}
	
}


function draw() {
	background(0);
	fill(255);
	text(currentMode, 50, 50);
}


function getIP(){
	return "128.122.6.191"
	// return ipInput.value();
}

let sendCandle = function(){
	sendMessage('C');
}

let sendTiming = function(){
	sendMessage('T');
}
let sendReading = function(){
	sendMessage('R');
}
let sendSunrise = function(){
	sendMessage('M');
}
let sendSunset = function(){
	sendMessage('E');
}
let sendDemo= function(){
	sendMessage('D');
}



function sendMessage(value){ 

	if(!waiting){
		waiting = true;

		console.log("KEY: " + value);
		let msg = url + getIP() + "/?"+value;
		console.log("Sending: " + msg);
		try{
			httpDo(
			    msg,
			    {
			      method: 'GET',
			      // Other Request options, like special headers for apis
			      headers: { 
			      	'Access-Control-Allow-Origin': '*',
			      	'Connection':"close"
			       }
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


