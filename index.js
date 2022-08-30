var TurndownService = require('turndown')
var process = require('process')
var fs = require('fs')

console.log(new TurndownService().turndown(fs.readFileSync(process.argv[2], 'utf8')))