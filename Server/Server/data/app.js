var connection = new WebSocket('ws://192.168.4.1:81/', ['arduino']);

connection.onopen = function () {
  connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
  if (e.data === '#3Alarm'){
      $('#alarm-container').addClass('bg-danger').removeClass('bg-success').removeClass('bg-warning');
  } else if (e.data === '#2Alarm') {
      $('#alarm-container').addClass('bg-warning').removeClass('bg-success').removeClass('bg-danger');
  } else if (e.data === '#1Normal') {
      $('#alarm-container').addClass('bg-success').removeClass('bg-danger').removeClass('bg-warning');
  }
  console.log('Server: ', e.data);
};
connection.onclose = function () {
  console.log('WebSocket connection closed');
};
