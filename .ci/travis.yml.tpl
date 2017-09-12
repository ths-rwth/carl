language: generic
sudo: false
dist: trusty

cache:
  apt: true
  directories:
    - $HOME/usr/
    - build/resources

addons:
  apt:
    sources: &sources_base
      - ubuntu-toolchain-r-test
    packages: &packages_base
      - g++-6
      - libcln-dev
      - libgmp-dev
      - libgtest-dev
      - openjdk-7-jdk
      - time
      - uuid-dev

matrix:
  include:
{%- for job in jobs %}
    - stage: {{ job.stage }}
      os: {{ job.os }}
  {%- if job.osx_image %}
      osx_image: {{ job.osx_image}}
  {%- endif %}
  {%- if job.env|length > 0 %}
      env: {{ job.env|join(' ') }}
  {%- endif %}
      script: {{ job.script|join(' ') }}
  {%- if job.apt_pkg %}
      addons:
        apt:
          sources: [{{ job.apt_src|join(', ') }}]
          packages: [{{ job.apt_pkg|join(', ') }}]
  {%- endif %}
{%- endfor %}

before_install:
  - cd .ci/ && source setup_travis.sh && cd ../

notifications:
  email:
    on_failure: always
    on_success: change
  irc:
    channels:
      - "chat.freenode.net#carl"
    template:
      - "Commit to %{repository_name}/%{branch} from %{author}"
      - "%{commit_message}"
      - "Build: %{message} %{duration} %{build_url}"
