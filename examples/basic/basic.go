package main

import (
	"encoding/json"
	"fmt"

	"github.com/tengattack/go-rapidjson"
)

func main() {
	s := `{"@timestamp":"2018-09-21T02:34:04.320Z","prospector":{"type":"log"}}`
	var v1 map[string]interface{}
	var d []byte
	err := rapidjson.UnmarshalString(s, &v1)
	if err != nil {
		panic(err)
	}
	d, _ = json.Marshal(v1)
	fmt.Printf("val: %#v %s\n", v1, d)

	s = "[-1,2,\"a\",{}]"
	var v2 []interface{}
	err = rapidjson.UnmarshalString(s, &v2)
	if err != nil {
		panic(err)
	}
	d, _ = json.Marshal(v2)
	fmt.Printf("val: %#v %s\n", v2, d)
}
