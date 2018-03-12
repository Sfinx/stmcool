#!/usr/bin/node
var m = require('moment');
console.log(m().unix() + m.parseZone().utcOffset() * 60);
