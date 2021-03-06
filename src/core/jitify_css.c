#define JITIFY_INTERNAL
#include "jitify_css.h"

extern int jitify_css_scan(jitify_lexer_t *lexer, const void *data, size_t length, int is_eof);

jitify_token_type_t jitify_type_css_selector = "CSS selector";
jitify_token_type_t jitify_type_css_term = "CSS term";
jitify_token_type_t jitify_type_css_comment = "CSS comment";
jitify_token_type_t jitify_type_css_optional_whitespace = "CSS optional space";
jitify_token_type_t jitify_type_css_required_whitespace = "CSS required space";
jitify_token_type_t jitify_type_css_url = "CSS URL";


static jitify_status_t css_transform(jitify_lexer_t *lexer, const void *data, size_t length, size_t offset)
{
  jitify_css_state_t *state = lexer->state;
  if (lexer->token_type == jitify_type_css_optional_whitespace) {
    if (lexer->remove_space) {
      return JITIFY_OK;
    }
  }
  else if (lexer->token_type == jitify_type_css_comment) {
    if (lexer->remove_comments) {
      return JITIFY_OK;
    }
  }
  else if ((lexer->token_type == jitify_type_css_selector) || (lexer->token_type == jitify_type_css_term)) {
    if (lexer->remove_space && (state->last_token_type == lexer->token_type)) {
      /* The space we just skipped was actually necessary, so add a space back in */
      if (jitify_write(lexer, " ", 1) < 0) {
        return JITIFY_ERROR;
      }
    }
  }
  state->last_token_type = lexer->token_type;
  if (jitify_write(lexer, data, length) < 0) {
    return JITIFY_ERROR;
  }
  else {
    return JITIFY_OK;
  }
}

static void css_cleanup(jitify_lexer_t *lexer)
{
  jitify_free(lexer->pool, lexer->state);
}

jitify_lexer_t *jitify_css_lexer_create(jitify_pool_t *pool, jitify_output_stream_t *out)
{
  jitify_lexer_t *lexer = jitify_lexer_create(pool, out);
  jitify_css_state_t *state = jitify_calloc(pool, sizeof(*state));
  lexer->state = state;
  lexer->scan = jitify_css_scan;
  lexer->transform = css_transform;
  lexer->cleanup = css_cleanup;
  return lexer;
}
