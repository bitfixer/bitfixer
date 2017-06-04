<?php
$verb = $_SERVER['REQUEST_METHOD'];
if ($verb == "GET")
{
	$path = $_GET['path'];
	$dir = "data" . $path;

	// check if the path points to a file
	if (is_file($dir))
	{
		// output the binary file
		header('Content-Type: application/octet-stream');
		$res = file_get_contents($dir);
		print $res;
	}
	elseif (is_dir($dir))
	{
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

		header('Content-Type: application/json');
		$result_json = json_encode($result);
		print $result_json;
	}
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
