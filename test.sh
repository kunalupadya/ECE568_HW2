curl --location --request POST 'http://httpbin.org/post' \
--header 'Connection: keep-alive' \
--header 'Cache-Control: max-age=0' \
--header 'Upgrade-Insecure-Requests: 1' \
--header 'Origin: http://httpbin.org' \
--header 'Content-Type: application/x-www-form-urlencoded' \
--header 'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.80 Safari/537.36' \
--header 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9' \
--header 'Referer: http://httpbin.org/forms/post' \
--header 'Accept-Language: en-US,en;q=0.9' \
--data-urlencode 'custname=' \
--data-urlencode 'custtel=' \
--data-urlencode 'custemail=' \
--data-urlencode 'delivery=' \
--data-urlencode 'comments=' \


