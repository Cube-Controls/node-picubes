{
  "targets": [
    {
      "target_name": "addon",
      "sources": [
        "addon.cc",
        "writedo.cc",
		"writeuo.cc",
		"readui.cc",
		"modbus.cc",
		"readcta.cc",
		"writecta.cc"
      ],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}
