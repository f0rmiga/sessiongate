# Image

The Docker image for the CircleCI pipeline.

## Building

```txt
cd $(git rev-parse --show-toplevel)/.circleci/image
./build.sh
```

## Testing

```txt
cd $(git rev-parse --show-toplevel)
circleci local execute --job "redis 4"
circleci local execute --job "redis 5"
```
