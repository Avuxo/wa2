// generate methods.json file based on subsection of d3d9helper.h
// this just creates the list of methods with args for both the class and class definitions files.
// assumes no whitespace or comments and only contents of DECLARE_INTERFACE_ macro

const { readFileSync, writeFileSync } = require('fs');

let header = readFileSync(process.argv[2]).toString();
header = header.split('\r\n');

const result = [];

for (definition of header) {
    if (definition.startsWith('STDMETHOD_')) { handleReturnMethod(definition); }
    else if (definition.startsWith('STDMETHOD')) { handleMethod(definition); }
    else { handleMemberVar(definition); }
}

writeFileSync('./methods.json', JSON.stringify(result, null, 2));

// handle STDMETHOD_ macro
function handleReturnMethod(def) {
    const returnType = def.match(/STDMETHOD_\((.*?),/)[1];
    def = def.replace('STDMETHOD_', '');
    def = def.replace(')(', ':');
    def = def.replace(/[\(\)]/g, '');
    def = def.replace(/ PURE;/, '');
    def = def.replace('THIS_ ', '')
    def = def.replace('THIS', 'void');

    const sig = def.split(':');

    const methodName = sig[0].split(',')[1].replace(' ', '');
    const params = sig[1];

    
    result.push({ returnType, methodName, params });
}

// handle STDMETHOD macro
function handleMethod(def) {
    const returnType = 'HRESULT';
    def = def.replace('STDMETHOD', '');
    def = def.replace(')(', ':');
    def = def.replace(/[\(\)]/g, '');
    def = def.replace(/ PURE;/, '');
    def = def.replace('THIS_ ', '')
    def = def.replace('THIS', 'void');

    const sig = def.split(':');

    const methodName = sig[0];
    const params = sig[1];
    
    result.push({ returnType, methodName, params });
}

// handle member variables
function handleMemberVar(def) {
    result.push(def);
}