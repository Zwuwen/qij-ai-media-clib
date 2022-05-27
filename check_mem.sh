rm -rf ./jpg/*
valgrind --tool=memcheck ./output/my-media ./config/config.json
