echo ">>> before install"

if [ -z "$TRAVIS_TAG"x ]; then
if [ "$DEPLOY_RELEASE"x = "true"x ]; then
  VALID=ok
fi
else
if [ "$RUN_COVERALLS"x = "true"x ]; then
  VALID=ok
fi
fi

if [ "$VALID"x != "ok"x ]; then
  exit
fi

sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
sudo apt-get update

echo "<<< before install"
