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
      osx_image: {{ job.osx_image }}
  {%- endif %}
  {%- if job.compiler %}
      compiler: {{ job.compiler }}
  {%- endif %}
  {%- if job.env|length > 0 %}
      env: {{ job.env|join(' ') }}
  {%- endif %}
      script: {{ job.script|join(' ') }}
  {%- if job.addons %}
      addons:
    {%- if job.addons.apt %}
        apt:
          sources: [{{ job.addons.apt.sources|join(', ') }}]
          packages: [{{ job.addons.apt.packages|join(', ') }}]
    {%- endif %}
    {%- if job.addons.coverity_scan %}
        coverity_scan:
          project:
            name: "{{ job.addons.coverity_scan.name }}"
            description: "{{ job.addons.coverity_scan.description }}"
      {%- for prop,value in job.addons.coverity_scan.properties.items() %}
          {{ prop }}: "{{ value }}"
      {%- endfor %}
    {%- endif %}
    {%- if job.addons.sonarcloud %}
        sonarcloud:
          organization: "{{ job.addons.sonarcloud.organization }}"
    {%- endif %}
  {%- endif %}
{%- endfor %}
  allow_failures:
{%- for job in jobs if job.allow_failure %}
    - stage: {{ job.stage }}
      os: {{ job.os }}
  {%- if job.osx_image %}
      osx_image: {{ job.osx_image}}
  {%- endif %}
  {%- if job.env|length > 0 %}
      env: {{ job.env|join(' ') }}
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
  slack:
    rooms:
      secure: eEyvqmcbM8Lwi1GJLXl/W1s+r9u/GHjsOjpawLE59jj553z3MWMAdOXnwnJfjRLiHQ8mWClFYK7wknwBOknWoIKAMe3nCYEtHMOaKLUQbxxeC8fWieQx1hivRXcJ8nH9o3gSDgc2YO5+1xfM9Eynsyf/geu+0UaWc2wHFModlQfkSIHO3gErVXqfdbrLvLUKeS3DAwOPIFBpok0L3wN4h+wlPmTE7rkVH9qgi377EzCVup+aMk8jj+sduhGVIjc81qPLmR1WxGNLSwGFY2UcVNaaWVZJ1tZZ/0IFxnUKtea0cmzaRtW5mfgQ2FsiR+ZIuWu09Rub9fEiShKqyj3YOXHJs1lNl731wIUAskDERCr/+tXQoF54fkQj9xBkyIAiZlceQ9CRHAvFD4dU+tfhZeYfOVWNf9hsUkdMo/c6ZcrQq7FTektKs8JT6Wx3lTxoWBaDhjsSaGMhNlaR7tx6myIIhAbNwQh5GgY6rKawDUq0hzg8O2MM3LxHNN8p7EbpOZZHWCjrx6ATTIMFD8MgfakpcExGjz4Qy+hQApIqE7C1zOmnTXEVtCkxP6TAQPwQHFwcdUKh7PXTu531nQXvulcVd3IHup1KsiLR6l4f3xxJqh57E/7gK9bLyxvIP75h7QzuJ9WQSKU+FOgt0vYO5b/Td8I0R+wL/tWzqWdhkfQ=
