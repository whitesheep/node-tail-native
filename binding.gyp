{
  "targets": [
    {
      "target_name": "tailnative",
      "sources": [
        "src/tail.cpp"
      ],
      'env' : {
        'CXX': 'g++'
      },
      'cflags': ['-fPIC', '-std=c99', '-pedantic', '-Wall']
    }
  ]
}
