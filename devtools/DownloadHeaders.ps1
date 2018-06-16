$ErrorActionPreference = "Stop"

docker build -t devtools:deps deps

$ContainerID = (docker run -d devtools:deps) | Select-Object -First 1

docker cp "$PSScriptRoot\ci\naemon\" "$($ContainerID):/usr/include/naemon/"
docker cp "$PSScriptRoot\ci\nagios\" "$($ContainerID):/usr/include/nagios/"
docker exec $ContainerID zip -r /headers.zip "/usr/include" "/usr/lib/gcc/x86_64-linux-gnu/7/include" "/usr/lib/gcc/x86_64-linux-gnu/7/include-fixed" "/usr/lib/x86_64-linux-gnu/glib-2.0/include" "/usr/local/include"
docker cp "$($ContainerID):/headers.zip" "$($PSScriptRoot)\headers.zip"
docker rm -f "$ContainerID"
