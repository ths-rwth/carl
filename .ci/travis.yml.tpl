language: generic
sudo: false
dist: trusty

cache:
  apt: true
  directories:
    - $HOME/usr/
    - $HOME/.sonar/cache
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
          token:
            secure: "{{ job.addons.sonarcloud.token }}"
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
      secure: "VSW+eUt5b9+280lJUmrV+1wOy6mYDG1efbYWu4rbOxzeZ+zOono4LuomxRktpbk5GgWf3123oe9/qGzFYUpHkBD2nK6OQVkNOsaCYtD2m8x8YFamLyGeFK+n9ml99SfhwAgVcdc+T4P2iVzthMMqcs0sgJY0vbSSXrwP1Npshkm7nXxh+ztA8PGBfrEtEN5e844AHP1JA7WnNQ0TEGLIPk06YkcIQ9ymfsg5U+n7/CdnW1Na8vZCWRJsnqymkrsLFyAxXNeBNLwN6C0L3LLImuJE84B2L4+UrGbwRZovAQWeNPgrcAt7J/vsEF4uvm7ei0+ZQOl9JG/ZkJu5/rj7Q62dGV2UdAHHPxU1heSw8faJrckKODZ6G3PwnPof4w3d9lX8wKEXD3SAkTWGmIoQbavB/3mBQhpXDDbfgRodDXTm/2q3SSLO9Unt7vzxurTD8cWWN3mvSInIAoliy2ZrdrwNLuHzEAPnY+CkRGQt2CUb0vXB3DNb5Gvnk7srbr/dPGYhUVkVuoR9hpS6w2WUWWi5M1RLoSopvjTd55gn5LLOFXvpMPacnV+aRXmCbGlrft8ezLcHep0adGYyn0YJbp9FsiislChhXCYbnchvjcKfS1aF7Nw0Tbrdb1ICBnbYOfLykwSe2S9ebYToMwqUpQruKPj1PXrQYB685EP9tno="
