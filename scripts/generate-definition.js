// generates actual class definition based on methods.json for wrapper

const { writeFileSync } = require('fs');
const methods = require('./methods.json');

let result = '';

for (method of methods) {
    if (typeof method != 'object') {
        continue;
    } else {
        result += `${method.returnType} NameSpace::${method.methodName}(${method.params}) {\r\n`;
        result += `    return orig->${method.methodName}(${genArgs(method.params)});\r\n`
        result += `}\r\n\r\n`;
    }
}

writeFileSync('class.cpp', result);

function genArgs(params) {
    if (params === 'void') return '';
    // get just names of params
    params = params.split(',');
    params = params.map(p => {
        p = p.split(' ');
        return p[p.length-1];
    });

    return params.join(', ');
}