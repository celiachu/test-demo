//数组中相同字母的次数
function fn(str) {
    var obj = {};
    str = str.toLowerCase();
    for (var i = 0; i < str.length; i++) {
        if (str[i] >= "a" && str[i] <= "z") {
            var key = str[i];
            if (obj[key]) {
                obj[key]++;
            }
            else {
                obj[key] = 1;
            }
        }
    }
    for (var key in obj) {
        console.log(key + ":" + obj[key]);
    }
}
var result = fn("AaBbc+b123d!@");