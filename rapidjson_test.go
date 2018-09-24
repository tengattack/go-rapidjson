package rapidjson_test

import (
	"encoding/json"
	"testing"

	jsoniter "github.com/json-iterator/go"
	"github.com/stretchr/testify/assert"
	. "github.com/tengattack/go-rapidjson"
)

type unmarshalTest struct {
	json string
	val  interface{}
}

var unmarshalTests = []unmarshalTest{
	{"null", nil},
	{"true", true},
	{"false", false},
	{"-1", int(-1)},
	{"-1234567890", int(-1234567890)},
	{"1", uint(1)},
	{"123", uint(123)},
	{"1234567890123", uint64(1234567890123)},
	{"-1234567890123", int64(-1234567890123)},
	{"1.23", float64(1.23)},
	{"\"123\"", "123"},
	// object
	{"{}", map[string]interface{}{}},
	{"{\"a\":123}", map[string]interface{}{"a": uint(123)}},
	{"{\"a\":1,\"b\":2,\"c\":3,\"d\":4,\"e\":5,\"f\":6,\"g\":7,\"h\":8,\"i\":9}",
		map[string]interface{}{
			"a": uint(1), "b": uint(2), "c": uint(3),
			"d": uint(4), "e": uint(5), "f": uint(6),
			"g": uint(7), "h": uint(8), "i": uint(9),
		}},
	{"{\"a\":{\"b\":123}}", map[string]interface{}{"a": map[string]interface{}{"b": uint(123)}}},
	// array
	{"[]", []interface{}{}},
	{"[\"a\",123]", []interface{}{"a", uint(123)}},
	// mixed object & array
	{"{\"a\":123,\"b\":[1,\"23\"]}", map[string]interface{}{
		"a": uint(123),
		"b": []interface{}{uint(1), "23"},
	}},
	{"[{},{\"a\":123,\"b\":\"foo\"},[]]", []interface{}{
		map[string]interface{}{},
		map[string]interface{}{"a": uint(123), "b": "foo"},
		[]interface{}{},
	}},
	{"{\"a\":[[{\"b\":123}]]}", map[string]interface{}{
		"a": []interface{}{
			[]interface{}{map[string]interface{}{"b": uint(123)}},
		},
	}},
}

func TestRapidJSON_Unmarshal(t *testing.T) {
	assert := assert.New(t)

	var v interface{}
	for _, test := range unmarshalTests {
		assert.NoError(Unmarshal([]byte(test.json), &v))
		assert.Equal(test.val, v)
	}
}

func TestRapidJSON_UnmarshalFast(t *testing.T) {
	assert := assert.New(t)

	var v interface{}
	for _, test := range unmarshalTests {
		assert.NoError(UnmarshalFast([]byte(test.json), &v))
		assert.Equal(test.val, v)
	}
}

func TestRapidJSON_UnmarshalString(t *testing.T) {
	assert := assert.New(t)

	var v interface{}
	for _, test := range unmarshalTests {
		assert.NoError(UnmarshalString(test.json, &v))
		assert.Equal(test.val, v)
	}
}

var largeJsonString = `[{
  "person": {
    "id": "d50887ca-a6ce-4e59-b89f-14f0b5d03b03",
    "name": {
      "fullName": "Leonid Bugaev",
      "givenName": "Leonid",
      "familyName": "Bugaev"
    },
    "email": "leonsbox@gmail.com",
    "gender": "male",
    "location": "Saint Petersburg, Saint Petersburg, RU",
    "geo": {
      "city": "Saint Petersburg",
      "state": "Saint Petersburg",
      "country": "Russia",
      "lat": 59.9342802,
      "lng": 30.3350986
    },
    "bio": "Senior engineer at Granify.com",
    "site": "http://flickfaver.com",
    "avatar": "https://d1ts43dypk8bqh.cloudfront.net/v1/avatars/d50887ca-a6ce-4e59-b89f-14f0b5d03b03",
    "employment": {
      "name": "www.latera.ru",
      "title": "Software Engineer",
      "domain": "gmail.com"
    },
    "facebook": {
      "handle": "leonid.bugaev"
    },
    "github": {
      "handle": "buger",
      "id": 14009,
      "avatar": "https://avatars.githubusercontent.com/u/14009?v=3",
      "company": "Granify",
      "blog": "http://leonsbox.com",
      "followers": 95,
      "following": 10
    },
    "twitter": {
      "handle": "flickfaver",
      "id": 77004410,
      "bio": null,
      "followers": 2,
      "following": 1,
      "statuses": 5,
      "favorites": 0,
      "location": "",
      "site": "http://flickfaver.com",
      "avatar": null
    },
    "linkedin": {
      "handle": "in/leonidbugaev"
    },
    "googleplus": {
      "handle": null
    },
    "angellist": {
      "handle": "leonid-bugaev",
      "id": 61541,
      "bio": "Senior engineer at Granify.com",
      "blog": "http://buger.github.com",
      "site": "http://buger.github.com",
      "followers": 41,
      "avatar": "https://d1qb2nb5cznatu.cloudfront.net/users/61541-medium_jpg?1405474390"
    },
    "klout": {
      "handle": null,
      "score": null
    },
    "foursquare": {
      "handle": null
    },
    "aboutme": {
      "handle": "leonid.bugaev",
      "bio": null,
      "avatar": null
    },
    "gravatar": {
      "handle": "buger",
      "urls": [
      ],
      "avatar": "http://1.gravatar.com/avatar/f7c8edd577d13b8930d5522f28123510",
      "avatars": [
        {
          "url": "http://1.gravatar.com/avatar/f7c8edd577d13b8930d5522f28123510",
          "type": "thumbnail"
        }
      ]
    },
    "fuzzy": false
  },
  "company": "hello"
}]`

func BenchmarkRapidJSON_Large_Unmarshal(b *testing.B) {
	b.SetBytes(int64(len(largeJsonString)))
	var v []interface{}
	for i := 0; i < b.N; i++ {
		Unmarshal([]byte(largeJsonString), &v)
	}
}

func BenchmarkRapidJSON_Large_UnmarshalFast(b *testing.B) {
	b.SetBytes(int64(len(largeJsonString)))
	var v interface{}
	for i := 0; i < b.N; i++ {
		UnmarshalFast([]byte(largeJsonString), &v)
	}
}

func BenchmarkJSONIter_Large_Unmarshal(b *testing.B) {
	b.SetBytes(int64(len(largeJsonString)))
	var v []interface{}
	for i := 0; i < b.N; i++ {
		jsoniter.Unmarshal([]byte(largeJsonString), &v)
	}
}

func BenchmarkStdJSON_Large_Unmarshal(b *testing.B) {
	b.SetBytes(int64(len(largeJsonString)))
	var v []interface{}
	for i := 0; i < b.N; i++ {
		json.Unmarshal([]byte(largeJsonString), &v)
	}
}

var smallJsonString = `{
	"github": {
    "handle": "buger",
    "id": 14009,
    "avatar": "https://avatars.githubusercontent.com/u/14009?v=3",
    "company": "Granify",
    "blog": "http://leonsbox.com",
    "followers": 95,
    "following": 10
  }
}`

func BenchmarkRapidJSON_Small_Unmarshal(b *testing.B) {
	b.SetBytes(int64(len(smallJsonString)))
	var v map[string]interface{}
	for i := 0; i < b.N; i++ {
		Unmarshal([]byte(smallJsonString), &v)
	}
}

func BenchmarkRapidJSON_Small_UnmarshalFast(b *testing.B) {
	b.SetBytes(int64(len(smallJsonString)))
	var v interface{}
	for i := 0; i < b.N; i++ {
		UnmarshalFast([]byte(smallJsonString), &v)
	}
}

func BenchmarkJSONIter_Small_Unmarshal(b *testing.B) {
	b.SetBytes(int64(len(smallJsonString)))
	var v map[string]interface{}
	for i := 0; i < b.N; i++ {
		jsoniter.Unmarshal([]byte(smallJsonString), &v)
	}
}

func BenchmarkStdJSON_Small_Unmarshal(b *testing.B) {
	b.SetBytes(int64(len(smallJsonString)))
	var v map[string]interface{}
	for i := 0; i < b.N; i++ {
		json.Unmarshal([]byte(smallJsonString), &v)
	}
}
