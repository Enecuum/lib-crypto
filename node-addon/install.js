var sys = require('sys')
var exec = require('child_process').exec;
function puts(error, stdout, stderr) { sys.puts(stdout) }

var os = require('os');
//control OS
//then run command depengin on the OS
console.log("Detecting OS and platform...");
console.log(os.type() + " " + os.arch());
if (os.type() === 'Linux'){
	console.log("Linux");
	if (os.arch() === 'ia32')
		console.log("ia32");
	else if (os.arch() === 'x64') 
		exec("npm install https://github.com/prudanoff/enecuum-bin/raw/master/bin/linux64/enecuum-crypto.tar.gz", puts);
}
   //exec("node build-linux.js", puts); 
else if (os.type() === 'Darwin') 
	console.log("Darwin");
   //exec("node build-mac.js", puts); 
else if (os.type() === 'Windows_NT'){
	if (os.arch() === 'ia32')
		console.log("ia32");
	else if (os.arch() === 'x64')
   		exec("npm install https://github.com/prudanoff/enecuum-bin/raw/master/bin/win64/enecuum-crypto.tar.gz", puts);
}
else
   throw new Error("Unsupported OS found: " + os.type());