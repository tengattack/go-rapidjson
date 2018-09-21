package main

import (
	"encoding/json"
	"fmt"

	"github.com/tengattack/go-rapidjson"
)

func main() {
	var v1 map[string]interface{}
	var d []byte
	err := rapidjson.UnmarshalString("{\"a\":-1,\"b\":[2]}", &v1)
	if err != nil {
		panic(err)
	}
	d, _ = json.Marshal(v1)
	fmt.Printf("val: %#v %s\n", v1, d)
	var v2 []interface{}
	err = rapidjson.UnmarshalString("[-1,2,\"a\",{}]", &v2)
	if err != nil {
		panic(err)
	}
	d, _ = json.Marshal(v2)
	fmt.Printf("val: %#v %s\n", v2, d)
}
