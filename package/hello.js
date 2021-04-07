var addon = require('bindings')('api');

console.log(addon.hello()); // 'world'
console.log(addon.query_camera_cpp(1));
