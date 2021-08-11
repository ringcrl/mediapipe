
eg:
	./scripts/example.sh
egseg:
	./scripts/egseg.sh
build-wasm:
	./scripts/build-wasm.sh
build:
	bazel build -c opt //hello-world:hello-world-simple --config=wasm
run-server:
	rm -f -r hello-server/*
	cp -r bazel-out/wasm-fastbuild/bin/hello-world/hello-world-wasm.js hello-server/
	cp -r bazel-out/wasm-fastbuild/bin/hello-world/hello-world-wasm.wasm hello-server/
	cp hello-world/*.html hello-server/
	python2 -m SimpleHTTPServer
run:
	rm -f -r hello-server/public/hello-world-simple.js hello-server/public/hello-world-simple.wasm hello-server/public/hello-world-simple.data
	cp -r bazel-out/wasm-opt/bin/hello-world/hello-world-simple.js hello-server/public/
	cp -r bazel-out/wasm-opt/bin/hello-world/hello-world-simple.wasm hello-server/public/
	cp -r bazel-out/wasm-opt/bin/hello-world/hello-world-simple.data hello-server/public/
	# python2 -m SimpleHTTPServer 8000
	./scripts/runserver.sh
	