// script to generate class definition for direct3d class wrapper

const { writeFileSync } = require('fs');
const methods = require('./methods.json');

// meant to be filled in later by hand;
let result = 'class abc : public Abc {\r\n';

for (method of methods) {
    if (typeof method != 'object') {
        result += `  ${method}\r\n`;
    } else {
        result += `  ${method.returnType} WINAPI ${method.methodName}(${method.params});\r\n`;
    }
}

result += '};'

writeFileSync('class.h', result);