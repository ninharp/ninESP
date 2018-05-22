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
    var max_zones      = 10;
    var wrapper         = $(".input_zones_wrap");
    var add_button      = $(".add_zone_button");
    
    var x = 1;
    $(add_button).click(function(e){ //on add input button click
        e.preventDefault();
        if(x < max_zones){ //max input box allowed
            x++; 
            $(wrapper).append('<div>Zone '+x+':<input type="text" class="form-control" style="width: 60px" name="zone'+x+'" value="0"/><a href="#" class="remove_field">Remove</a></div>');
				$('#zone-count').text('Zone Count: '+x);
        }
    });
    
    $('#zone-count').text('Zone Count: '+x);
    
    $(wrapper).on("click",".remove_field", function(e){ //user click on remove text
        e.preventDefault(); $(this).parent('div').remove(); x--;
    })
});
