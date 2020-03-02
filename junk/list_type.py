import os

def is_str(data):
    return "\""+data+"\""

def process_type(data, __type):
    type_list = {
        "text"            :    "text",
        "text-hex"        :    "string",
        "boolean"         :    "boolean",
        "ip-address"      :    "ip-address",
        "ip-address-list" :    "array of ip-address"
    }

    type_rules = {
        "text"            :    is_str(data),
        "text-hex"        :    is_str(data),
        "boolean"         :    data,
        "ip-address"      :    data,
        "ip-address-list" :    data
    }

    return type_rules[__type]

data = 'deadbeef'
print data
print process_type(data, 'ip-address-list')
