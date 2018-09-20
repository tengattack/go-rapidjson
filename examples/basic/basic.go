package main

import (
	"encoding/json"
	"fmt"

	"github.com/tengattack/go-rapidjson"
)

func main() {
	var v interface{}
	var d []byte
	err := rapidjson.UnmarshalString("{\"a\":-1,\"b\":[2]}", &v)
	if err != nil {
		panic(err)
	}
	d, _ = json.Marshal(v)
	fmt.Printf("val: %#v %s\n", v, d)
	err = rapidjson.UnmarshalString("[-1,2,\"a\",{}]", &v)
	if err != nil {
		panic(err)
	}
	d, _ = json.Marshal(v)
	fmt.Printf("val: %#v %s\n", v, d)
}
