var addon = require('bindings')('api');

try{
console.log(addon.hello()); // 'world'
console.log(addon.query_camera_cpp(1));
console.log(addon.query_camera_cpp(1));
console.log(addon.query_camera_cpp(1));
}catch(exception){
    console.log(exception)
}
