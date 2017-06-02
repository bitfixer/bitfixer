<?php
$verb = $_SERVER['REQUEST_METHOD'];
if ($verb == "GET")
{
	$dir = "data";
	$entries = scandir($dir);
	$result = array();

	foreach ($entries as $key => $value)
	{
		if (!in_array($value,array(".",".."))) 
		{ 
			if (is_dir($dir . DIRECTORY_SEPARATOR . $value)) 
			{ 
				$result[$value] = dirToArray($dir . DIRECTORY_SEPARATOR . $value); 
			} 
			else 
			{ 
				$entry = array();
				$entry['name'] = $value;
				$entry['path'] = $dir . DIRECTORY_SEPARATOR . $value;
				$result[] = $entry;
			} 
		} 
	}

	//print_r($result);
	$result_json = json_encode($result);
	print $result_json;


	//$files_json = json_encode($files);
	//print $files_json;
}
else if ($verb == "PUT")
{
	$path = $_GET['path'];
	print "path: $path ";

	$handle = fopen('php://input', 'r');
	$rawData = '';
	while ($chunk = fread($handle, 1024)) {
		$rawData .= $chunk;
	}

	$res = file_put_contents($path, $rawData);
	print "res $res";
}

?>