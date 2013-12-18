<!DOCTYPE html>
<html>
<head>
<title>"Mask" Configuration</title>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.css" type="text/css">
<script src="http://code.jquery.com/jquery-1.9.1.min.js" type="text/javascript"></script>
<script src="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.js" type="text/javascript"></script>
</head>

<body>
    <div data-role="page" id="main">
        <div data-role="header" class="jqm-header">
            <h1>Configure "Mask"</h1>
        </div>

        <div data-role="content">
            <div data-role="fieldcontain">
                <label for="slide-theme">Theme</label>
                <select name="slide-theme" id="slide-theme" data-role="slider" data-theme="c" data-track-theme="a">
                    <option value="dark"<?php echo $_GET['theme'] == 'dark' ? ' selected' : ''; ?>>
                        Dark
                    </option>
                    <option value="light"<?php echo $_GET['theme'] == 'light' ? ' selected' : ''; ?>>
                        Light
                    </option>
                </select>
            </div>

            <div class="ui-body ui-body-b">
                <fieldset class="ui-grid-a">
                    <div class="ui-block-a">
                        <button type="submit" data-theme="d" id="b-cancel">Cancel</button>
                    </div>

                    <div class="ui-block-b">
                        <button type="submit" data-theme="a" id="b-submit">Submit</button>
                    </div>
                </fieldset>
            </div>
        </div>
    </div>
    
    <script type="text/javascript">
		function saveOptions() {
			var options = {
				"0": $("#slide-theme").val()
			};

			return options;
		}

		$().ready(function() {
			$("#b-cancel").click(function() {
				window.location.href = "pebblejs://close#failure";
			});

			$("#b-submit").click(function() {
				var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
				window.location.href = location;
			});
		});
    </script>
</body>
</html>