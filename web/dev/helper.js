  $(window).on('pageshow',function() { 
		if ($('#relay').is(':checked')) {
				$('#relay-group').show();
			} else {
				$('#relay-group').hide();
			}   
			if ($('#adc').is(':checked')) {
				$('#adc-group').show();
			} else {
				$('#adc-group').hide();
			} 	
			if ($('#rcswitch').is(':checked')) {
				$('#rcswitch-group').show();
			} else {
				$('#rcswitch-group').hide();
			} 
			if ($('#motion').is(':checked')) {
				$('#motion-group').show();
			} else {
				$('#motion-group').hide();
			} 
			if ($('#max7219').is(':checked')) {
				$('#max7219-group').show();
			} else {
				$('#max7219-group').hide();
			}  
			
			
	});
	$(window).on('load', function() {
    	$('#relay').click(function() {
			if ($('#relay').is(':checked')) {
				$('#relay-group').show();
			} else {
				$('#relay-group').hide();
			}   		
    	});
    	$('#adc').click(function() {
			if ($('#adc').is(':checked')) {
				$('#adc-group').show();
			} else {
				$('#adc-group').hide();
			}   		
    	});
    	$('#rcswitch').click(function() {
			if ($('#rcswitch').is(':checked')) {
				$('#rcswitch-group').show();
			} else {
				$('#rcswitch-group').hide();
			}   		
    	});
    	$('#motion').click(function() {
			if ($('#motion').is(':checked')) {
				$('#motion-group').show();
			} else {
				$('#motion-group').hide();
			}   		
    	});
    	$('#max7219').click(function() {
			if ($('#max7219').is(':checked')) {
				$('#max7219-group').show();
			} else {
				$('#max7219-group').hide();
			}   		
    	});
	});
	
$(document).ready(function() {
    var max_fields      = 10;
    var wrapper         = $(".zone_container");
    var add_button      = $(".add_zone_field");
    var submit_button	= $(".btn-success");
    var zones				= $('input[name=max7219_zones]').val();
    if (zones <= 0) {
    	zones = 1;
    } else {
    	var i;
    	for(i = 1; i < zones; i++) {
         $(wrapper).append('<div><font class="zone_text">Zone '+i+'</font><input class="zone_num" type="text" name="max7219_zone'+i+'" value="{max7219_zone'+i+'}"/><font class="zone_text">'+$('input[name=max7219_prefix]').val()+i+'/#'+'</font><a href="#" class="delete">Delete Zone '+i+'</a></div>');       
    	}
    	$('input[name=max7219_zones]').attr('value', zones);
    }
  		
    var x = zones;
        
    $(add_button).click(function(e){
        e.preventDefault();
        if(x < max_fields){
            x++;
            $(wrapper).append('<div><font class="zone_text">Zone '+(x-1)+'</font><input class="zone_num" type="text" name="max7219_zone'+(x-1)+'" value="'+zones+'"/><font class="zone_text">'+$('input[name=max7219_prefix]').val()+(x-1)+'/#'+'</font><a href="#" class="delete">Delete Zone '+(x-1)+'</a></div>');
            $("input[name=max7219_zones]").attr('value', x);
        }
  		  else
  		  {
  		  	  alert('You Reached the zone count limits')
  		  }
    });
    
     // Check for reasonable inputs
    $(submit_button).click(function(e){
        e.preventDefault();
        var ret = 0;
        
        // Check if zones are correctly assigned
		  zones = $("input[name=max7219_zones]").val();
		  var devices = $("input[name=max7219_count]").val();
		  var count = 0;
		  var i;
		  for (i = 0; i < zones; i++) {
		  		count = count+parseInt($("input[name=max7219_zone"+i+"]").val());
		  }
		  if (count != devices) {
		  		alert("Devices are not evenly allocated!\nDevices: "+devices+' - Count in Zones: '+count);
		  		ret = 1;
		  }
			
		  if (ret == 0) {
			  $("#periph").submit();
		  }
    });
  
	
    $(wrapper).on("click",".delete", function(e){
        e.preventDefault(); $(this).parent('div').remove(); x--;
        $("input[name=max7219_zones]").attr('value', x);
    })
});
