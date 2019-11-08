language: generic
dist: bionic

cache:
  apt: true
  directories:
{%- for cache in cached %}
    - {{ cache }}
{%- endfor %}

addons:
  apt:
    sources_base: &sources_base
      sourceline: 'ppa:ubuntu-toolchain-r/test'
    packages: &packages_base
      - texinfo
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
  {%- if job.if %}
      if: {{ job.if }}
  {%- endif %}
  {%- if job.script %}
      script: {{ job.script|join(' ') }}
  {%- endif %}
  {%- if job.addons %}
    {%- if job.addons.cache %}
      cache:
        apt: true
        directories:
      {%- for dir in job.addons.cache %}
          - {{ dir }}
      {%- endfor %}
    {%- endif %}
      addons:
    {%- if job.addons.apt %}
      {%- set sources = ["*sources_base"] + job.addons.apt.sources %}
      {%- set packages = ["*packages_base"] + job.addons.apt.packages %}
        apt:
          sources:
      {%- for s in sources %}
        {%- if s is mapping %}
          - sourceline: '{{ s.sourceline }}'
            key_url: {{ s.key_url }}
        {%- else %}
          - {{ s }}
        {%- endif %}
      {%- endfor %}
          packages: [{{ packages|join(', ') }}]
    {%- endif %}
    {%- if job.addons.homebrew %}
        homebrew:
          packages:
      {%- for p in job.addons.homebrew.packages %}
          - {{ p }}
      {%- endfor %}
          update: true
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
      secure: "mL8Ku2omjsseTL5P2R2P3ddCedg8u9qSTKRjKNBhOYRJvpKNrIWr9fouP1tGsah9GBF+HL/JFWlA2sckCL9pURx6TdICHSgik1V/48QhpPKrV+ZkCmKenw3+hdXQY3YP36Xpj/SCCSMyRovA6kacWL3k0gRGJ++dxZYrYF7oVd8ltwxNC6+1o9HCmJnueCME5GBCKCgtOhYOr7izkjQ9wtFJvCmxHcI9EZxMQjWXmcHfItaWD1YFhR0/7AbsxJ9xtYZD0OMJrOgRBau671wV603a0EctIPcv0uKzjSsvB3yLndGOcMRsA8x/OPrFoOHbpLZ9d4O9gY3lvW5oueV+KTbbvHMSxJtvB9q3F0ZJSDHoG7cn1E2xb4gOHRdu4FhwqYy9abUM0DVUI/f7GaL+DMgzKeTduLB6h4GhtGR7fcWaBpCJYN1k8o33Vf/OPwaoPVCeiDRQIFqrqjDRcBc7ONe096WAf61albpMW6iCr5cBHy/j9XqEAGHu6sYftogWm/YVRqSF9+vnZy41cfQU+SYlFtWusUepqX1X/Kei+MCwESuddBnWj+oX8N8nreI8ql2IHG79IQVeEZZKMowxmo2IMsNnvkMyRTQsOmFY2u4NMRGga71QRCSbNvl01KxQ2ilyQ8Gu9Kjbs1QJ76jhOutChCzwOBAgc5aREdNCaV0="
